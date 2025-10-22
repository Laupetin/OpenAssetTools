<script setup lang="ts">
import { ref, watch } from "vue";
import { useZoneStore } from "@/stores/ZoneStore";
import ZoneInspectorDetails from "./ZoneInspectorDetails.vue";
import ZoneInspectorZoneList from "./ZoneInspectorZoneList.vue";

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
  <div class="zone-inspector">
    <ZoneInspectorZoneList v-model:selected-zone="selectedZone" />
    <ZoneInspectorDetails :selected-zone="selectedZone" />
  </div>
</template>

<style lang="scss" scoped>
.zone-inspector {
  display: flex;
  flex-direction: row;
  width: 100%;

  & > * {
    width: 50%;
    padding: 1rem 2rem;
  }
}
</style>
