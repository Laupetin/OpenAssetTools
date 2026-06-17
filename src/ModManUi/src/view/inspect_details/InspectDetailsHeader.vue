<script setup lang="ts">
import { computed } from "vue";
import type { ZoneDto } from "@/native/ZoneBinds.ts";
import { useZoneStore } from "@/stores/ZoneStore.ts";
import Tag from "primevue/tag";
import { localizeGame, localizePlatform } from "@/i18n/i18n.ts";

const zoneStore = useZoneStore();
const props = defineProps<{
  zoneName: string;
}>();

const zoneDetails = computed<ZoneDto | null>(() =>
  props.zoneName ? zoneStore.getLoadedZoneByName(props.zoneName) : null,
);
</script>

<template>
  <span>
    <span>Inspect zone: {{ zoneName }}</span>
    <template v-if="zoneDetails">
      <Tag class="zone-header-tag" :value="localizeGame(zoneDetails.game)" severity="secondary" />
      <Tag
        class="zone-header-tag"
        :value="localizePlatform(zoneDetails.platform)"
        severity="secondary"
      />
    </template>
  </span>
</template>

<style scoped lang="scss">
.zone-header-tag {
  margin-left: 0.5em;
}
</style>
