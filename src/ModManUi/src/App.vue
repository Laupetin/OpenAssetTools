<script setup lang="ts">
import Button from "primevue/button";
import { computed, ref } from "vue";
import { webviewAddEventListener, webviewBinds } from "@/native";
import ProgressBar from "primevue/progressbar";
import ZoneSelector from "./components/ZoneSelector.vue";

const loadingFastFile = ref(false);
const unlinkingFastFile = ref(false);
const lastPercentage = ref<number>(0);

const performingAction = computed<boolean>(() => loadingFastFile.value || unlinkingFastFile.value);

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
      lastPercentage.value = 100;
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
    lastPercentage.value = 100;
  }
}

webviewAddEventListener("zoneLoadProgress", (dto) => {
  lastPercentage.value = Math.floor(dto.percentage * 1000) / 10;
});

webviewAddEventListener("zoneUnlinkProgress", (dto) => {
  lastPercentage.value = Math.floor(dto.percentage * 1000) / 10;
});
</script>

<template>
  <main class="container">
    <h1>Welcome to ModMan</h1>
    <small>Nothing to see here yet, this is mainly for testing</small>

    <div class="actions">
      <Button
        label="Load fastfile"
        :disabled="performingAction"
        :loading="loadingFastFile"
        @click="onOpenFastFileClick"
      />
      <Button
        label="Unlink fastfile"
        :disabled="performingAction"
        :loading="unlinkingFastFile"
        @click="onUnlinkFastFileClick"
      />
    </div>

    <ZoneSelector />

    <ProgressBar
      v-if="performingAction"
      class="progressbar"
      :show-value="false"
      :value="lastPercentage"
    />
  </main>
</template>

<style scoped lang="scss">
.actions {
  display: flex;
  justify-content: center;
  column-gap: 0.5em;
}

.zone-list {
  display: flex;
  flex-direction: column;
  row-gap: 0.5em;
}

.zone > button {
  margin-left: 0.5em;
}

@starting-style {
  .progressbar {
    opacity: 0;
  }
}

.progressbar {
  position: absolute;
  bottom: 0;
  left: 0;
  right: 0;

  height: 0.4rem;

  transition: opacity 0.2s ease-in-out;
  opacity: 1;
}
</style>
