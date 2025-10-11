<script setup lang="ts">
import { ref } from "vue";
import { webviewBinds } from "@/native";
import { useZoneStore } from "@/stores/ZoneStore";
import SpinningLoader from "@/components/SpinningLoader.vue";

const zoneStore = useZoneStore();
const lastPath = ref("");
const loadingFastFile = ref(false);

async function onOpenFastfileClick() {
  lastPath.value =
    (await webviewBinds.openFileDialog({ filters: [{ name: "Fastfiles", filter: "*.ff" }] })) ?? "";

  loadingFastFile.value = true;

  webviewBinds
    .loadFastFile(lastPath.value)
    .catch((e: string) => {
      console.error("Failed to load fastfile:", e);
    })
    .finally(() => {
      loadingFastFile.value = false;
    });
}

function onUnloadClicked(zoneName: string) {
  webviewBinds.unloadZone(zoneName).catch((e: string) => {
    console.error("Failed to unload zone:", e);
  });
}
</script>

<template>
  <main class="container">
    <h1>Welcome to ModMan</h1>
    <small>Nothing to see here yet, this is mainly for testing</small>

    <p>
      <button :disabled="loadingFastFile" @click="onOpenFastfileClick">
        <SpinningLoader v-if="loadingFastFile" class="loading" />
        <span>Load fastfile</span>
      </button>
    </p>
    <div>
      <h3>Loaded zones:</h3>
      <div class="zone-list">
        <div v-for="zone in zoneStore.loadedZones" :key="zone" class="zone">
          <span>{{ zone }}</span>
          <button @click="onUnloadClicked(zone)">Unload</button>
        </div>
      </div>
    </div>
  </main>
</template>

<style scoped>
.loading {
  margin-right: 0.2em;
}

.zone-list {
  display: flex;
  flex-direction: column;
  row-gap: 0.5em;
}

.zone > button {
  margin-left: 0.5em;
}
</style>
