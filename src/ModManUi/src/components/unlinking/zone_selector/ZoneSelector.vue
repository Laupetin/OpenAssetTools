<script setup lang="ts">
import { ref, watch } from "vue";
import { useZoneStore } from "@/stores/ZoneStore";
import ZoneSelectorDetails from "./ZoneSelectorDetails.vue";
import ZoneSelectorZoneList from "./ZoneSelectorZoneList.vue";

const zoneStore = useZoneStore();
const selectedZone = ref<string | null>(null);

watch(
  () => zoneStore.loadedZones,
  (newValue) => {
    // Reset selection if unloaded
    if (!selectedZone.value) return;
    if (newValue.findIndex((loadedZone) => loadedZone.name === selectedZone.value) >= 0) return;
    selectedZone.value = null;
  },
  { deep: true },
);
</script>

<template>
  <div class="zone-selector">
    <ZoneSelectorZoneList v-model:selected-zone="selectedZone" />

    <ZoneSelectorDetails :selected-zone="selectedZone" />
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
