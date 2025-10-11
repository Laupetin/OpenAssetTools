<script setup lang="ts">
import { ref } from "vue";
import { webviewBinds } from "@/native";
import { useZoneStore } from "@/stores/ZoneStore";

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
      <button @click="onOpenFastfileClick">Open fastfile</button>
      <span>Loading: {{ loadingFastFile }}</span>
    </p>
    <div>
      <h3>Loaded zones:</h3>
      <div v-for="zone in zoneStore.loadedZones" :key="zone">
        <span>{{ zone }}</span>
        <button @click="onUnloadClicked(zone)">Unload</button>
      </div>
    </div>
  </main>
</template>

<style scoped>
.logo.vite:hover {
  filter: drop-shadow(0 0 2em #747bff);
}

.logo.vue:hover {
  filter: drop-shadow(0 0 2em #249b73);
}
</style>
