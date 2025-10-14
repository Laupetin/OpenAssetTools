<script setup lang="ts">
import { computed, ref } from "vue";
import { webviewAddEventListener, webviewBinds } from "@/native";
import { useZoneStore } from "@/stores/ZoneStore";
import SpinningLoader from "@/components/SpinningLoader.vue";

const zoneStore = useZoneStore();
const loadingFastFile = ref(false);
const unlinkingFastFile = ref(false);
const lastPercentage = ref<number>(0);

const performingAction = computed<boolean>(() => loadingFastFile.value || unlinkingFastFile.value);
const progressBarWidth = computed<string>(() => `${lastPercentage.value * 100}%`);

async function openFastFileSelect() {
  return await webviewBinds.openFileDialog({ filters: [{ name: "Fastfiles", filter: "*.ff" }] });
}

async function onOpenFastFileClick() {
  if (performingAction.value) return;

  const fastFilePath = await openFastFileSelect();
  if (!fastFilePath) return;

  loadingFastFile.value = true;
  lastPercentage.value = 0;

  webviewBinds
    .loadFastFile(fastFilePath)
    .catch((e: string) => {
      console.error("Failed to load fastfile:", e);
    })
    .finally(() => {
      loadingFastFile.value = false;
      lastPercentage.value = 1;
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
      lastPercentage.value = 0;
      loadedZoneName = (await webviewBinds.loadFastFile(fastFilePath)).zoneName;
    } catch (e: unknown) {
      console.error("Failed to load fastfile:", e as string);
      return;
    }

    try {
      lastPercentage.value = 0;
      await webviewBinds.unlinkZone(loadedZoneName);
    } catch (e: unknown) {
      console.error("Failed to unlink fastfile:", e as string);
      return;
    } finally {
      webviewBinds.unloadZone(loadedZoneName);
    }
  } finally {
    unlinkingFastFile.value = false;
    lastPercentage.value = 1;
  }
}

function onUnloadClicked(zoneName: string) {
  webviewBinds.unloadZone(zoneName).catch((e: string) => {
    console.error("Failed to unload zone:", e);
  });
}

webviewAddEventListener("zoneLoadProgress", (dto) => {
  lastPercentage.value = dto.percentage;
});

webviewAddEventListener("zoneUnlinkProgress", (dto) => {
  lastPercentage.value = dto.percentage;
  console.log("unlink", dto);
});
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

    <div class="progressbar-wrapper">
      <div
        class="progressbar"
        :class="{ visible: performingAction }"
        :style="{ width: progressBarWidth }"
      ></div>
    </div>
  </main>
</template>

<style scoped lang="scss">
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

.progressbar-wrapper {
  position: absolute;
  bottom: 0;
  left: 0;
  right: 0;
  padding: 0.35rem 0.4rem;
}

.progressbar {
  opacity: 0;
  height: 0.4rem;
  border-radius: 2.5rem;
  background-color: #b9772c;
  transition: opacity 0.2s ease-in-out;

  &.visible {
    opacity: 1;
  }
}
</style>
