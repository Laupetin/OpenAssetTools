<script setup lang="ts">
import Button from "primevue/button";
import Listbox from "primevue/listbox";
import { ref, watch } from "vue";
import { useZoneStore } from "@/stores/ZoneStore";
import { webviewBinds } from "@/native";

const zoneStore = useZoneStore();
const selectedZone = ref<string | null>(null);

function onUnloadClicked() {
  if (!selectedZone.value) return;

  webviewBinds.unloadZone(selectedZone.value).catch((e: string) => {
    console.error("Failed to unload zone:", e);
  });
}

watch(
  () => zoneStore.loadedZones,
  (newValue) => {
    // Reset selection if unloaded
    if (!selectedZone.value) return;
    if (newValue.indexOf(selectedZone.value) >= 0) return;
    selectedZone.value = null;
  },
  { deep: true },
);
</script>

<template>
  <div class="zone-selector">
    <div class="zone-list">
      <Listbox v-model="selectedZone" :options="zoneStore.loadedZones" class="zone" />
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
</style>
