<script setup lang="ts">
import type { ZoneDto } from "@/native/ZoneBinds";
import { useZoneStore } from "@/stores/ZoneStore";
import { computed } from "vue";

const zoneStore = useZoneStore();

const props = defineProps<{
  selectedZone: string;
}>();

const selectedZoneDetails = computed<ZoneDto | null>(
  () => zoneStore.loadedZones.find((zone) => zone.name === props.selectedZone) ?? null,
);
</script>

<template>
  <div class="zone-details">
    <h2>{{ selectedZone ?? "No zone selected" }}</h2>
  </div>
</template>

<style lang="scss" scoped>
.zone-details {
}
</style>
