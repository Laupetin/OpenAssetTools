<script setup lang="ts">
import { computed, ref } from "vue";
import { webviewBinds } from "@/native";
import { useZoneStore } from "@/stores/ZoneStore";
import SpinningLoader from "@/components/SpinningLoader.vue";

const zoneStore = useZoneStore();
const loadingFastFile = ref(false);
const unlinkingFastFile = ref(false);

const performingAction = computed<boolean>(() => loadingFastFile.value || unlinkingFastFile.value);

async function openFastFileSelect() {
  return await webviewBinds.openFileDialog({ filters: [{ name: "Fastfiles", filter: "*.ff" }] });
}

async function onOpenFastFileClick() {
  if (performingAction.value) return;

  const fastFilePath = await openFastFileSelect();
  if (!fastFilePath) return;

  loadingFastFile.value = true;

  webviewBinds
    .loadFastFile(fastFilePath)
    .catch((e: string) => {
      console.error("Failed to load fastfile:", e);
    })
    .finally(() => {
      loadingFastFile.value = false;
    });
}

async function onUnlinkFastFileClick() {
  if (performingAction.value) return;

  const fastFilePath = await openFastFileSelect();
  if (!fastFilePath) return;

  try {
    unlinkingFastFile.value = true;

    let loadedZoneName: string;
    try {
      loadedZoneName = (await webviewBinds.loadFastFile(fastFilePath)).zoneName;
    } catch (e: unknown) {
      console.error("Failed to load fastfile:", e as string);
      return;
    }

    try {
      await webviewBinds.unlinkZone(loadedZoneName);
    } catch (e: unknown) {
      console.error("Failed to unlink fastfile:", e as string);
      return;
    } finally {
      webviewBinds.unloadZone(loadedZoneName);
    }
  } finally {
    unlinkingFastFile.value = false;
  }
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

    <div class="actions">
      <button :disabled="performingAction" @click="onOpenFastFileClick">
        <SpinningLoader v-if="loadingFastFile" class="loading" />
        <span>Load fastfile</span>
      </button>
      <button :disabled="performingAction" @click="onUnlinkFastFileClick">
        <SpinningLoader v-if="unlinkingFastFile" class="loading" />
        <span>Unlink fastfile</span>
      </button>
    </div>

    <div>
      <h3>Loaded zones:</h3>
      <div class="zone-list">
        <div v-for="zone in zoneStore.loadedZones" :key="zone" class="zone">
          <span>{{ zone }}</span>
          <button :disabled="performingAction" @click="onUnloadClicked(zone)">Unload</button>
        </div>
      </div>
    </div>
  </main>
</template>

<style scoped>
.actions {
  display: flex;
  justify-content: center;
  column-gap: 0.5em;
}

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
