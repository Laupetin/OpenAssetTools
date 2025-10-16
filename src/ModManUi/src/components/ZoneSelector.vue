<script setup lang="ts">
import Button from "primevue/button";
import ProgressBar from "primevue/progressbar";
import Listbox from "primevue/listbox";
import { computed, ref, watch } from "vue";
import { useZoneStore } from "@/stores/ZoneStore";
import { webviewBinds } from "@/native";

interface SelectableZone {
  isLoading: boolean;
  zoneName: string;
}

const zoneStore = useZoneStore();
const selectedZone = ref<SelectableZone | null>(null);

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

  webviewBinds.unloadZone(selectedZone.value.zoneName).catch((e: string) => {
    console.error("Failed to unload zone:", e);
  });
}

watch(
  () => zoneStore.loadedZones,
  (newValue) => {
    // Reset selection if unloaded
    if (!selectedZone.value) return;
    if (newValue.findIndex((loadedZone) => loadedZone.name === selectedZone.value?.zoneName) >= 0)
      return;
    selectedZone.value = null;
  },
  { deep: true },
);
</script>

<template>
  <div class="zone-selector">
    <div class="zone-list">
      <Listbox
        v-model="selectedZone"
        :options="availableZones"
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

    <div class="zone-actions">
      <Button label="Unload" :disabled="!selectedZone" @click="onUnloadClicked" />
    </div>
  </div>
</template>

<style lang="scss" scoped>
.zone-selector {
  display: flex;
  flex-direction: row;
  width: 100%;

  & > * {
    width: 50%;
    padding: 1rem 2rem;
  }
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
