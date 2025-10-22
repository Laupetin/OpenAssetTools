<script setup lang="ts">
import Button from "primevue/button";
import ProgressBar from "primevue/progressbar";
import Listbox from "primevue/listbox";
import { computed } from "vue";
import { useZoneStore } from "@/stores/ZoneStore";
import { webviewBinds } from "@/native";

interface SelectableZone {
  isLoading: boolean;
  zoneName: string;
}

const zoneStore = useZoneStore();
const selectedZone = defineModel<string | null>("selectedZone");

async function openFastFileSelect() {
  return await webviewBinds.openFileDialog({ filters: [{ name: "Fastfiles", filter: "*.ff" }] });
}

async function onOpenFastFileClick() {
  const fastFilePath = await openFastFileSelect();
  if (!fastFilePath) return;

  zoneStore.loadFastFile(fastFilePath);
}

const availableZones = computed<SelectableZone[]>(() => {
  const result = [
    ...zoneStore.zonesCurrentlyBeingLoaded.map(
      (zoneBeingLoaded) =>
        ({
          isLoading: true,
          zoneName: zoneBeingLoaded,
        }) satisfies SelectableZone,
    ),
    ...zoneStore.loadedZones.map(
      (loadedZone) =>
        ({
          isLoading: false,
          zoneName: loadedZone.name,
        }) satisfies SelectableZone,
    ),
  ];

  result.sort((a, b) => a.zoneName.localeCompare(b.zoneName));

  return result;
});

function onUnloadClicked() {
  if (!selectedZone.value) return;

  webviewBinds.unloadZone(selectedZone.value).catch((e: string) => {
    console.error("Failed to unload zone:", e);
  });
}
</script>

<template>
  <div class="zone-list">
    <div class="zone-list-actions">
      <Button label="Load fastfile" @click="onOpenFastFileClick" />
      <Button label="Unload" :disabled="!selectedZone" @click="onUnloadClicked" />
    </div>
    <Listbox
      v-model="selectedZone"
      :options="availableZones"
      option-disabled="isLoading"
      option-value="zoneName"
      data-key="zoneName"
      emptyMessage="No zones loaded"
      class="zone"
    >
      <template #option="{ option }: { option: SelectableZone }">
        <div class="selectable-zone">
          <span>{{ option.zoneName }}</span>
          <ProgressBar
            v-if="option.isLoading"
            class="zone-progressbar"
            :value="zoneStore.getPercentageForZoneBeingLoaded(option.zoneName)"
            :show-value="false"
          />
        </div>
      </template>
    </Listbox>
  </div>
</template>

<style lang="scss" scoped>
.zone-list-actions {
  display: flex;
  column-gap: 0.25em;
  row-gap: 0.25em;
  padding: 0.25em 0;
}

.selectable-zone {
  position: relative;
  text-align: left;
  width: 100%;
}

.zone-progressbar {
  position: absolute;
  left: 0;
  bottom: 0;
  right: 0;
  height: 0.2em;
}
</style>
