<script setup lang="ts">
import Button from "primevue/button";
import Tag from "primevue/tag";
import MeterGroup, { type MeterItem } from "primevue/metergroup";
import Skeleton from "primevue/skeleton";
import { dt } from "@primeuix/themes";
import type { ZoneDto } from "@/native/ZoneBinds";
import { useZoneStore } from "@/stores/ZoneStore";
import { computed, ref, watch } from "vue";
import type { CommonAssetType, ZoneAssetsDto } from "@/native/AssetBinds";
import { webviewBinds } from "@/native";
import { getAssetTypeNameCapitalized } from "@/utils/AssetTypeName";

const zoneStore = useZoneStore();

const props = defineProps<{
  selectedZone: string | null;
}>();

const assets = ref<ZoneAssetsDto | null>(null);
const assetCount = computed(() => assets.value?.assets.length ?? 0);
const referenceCount = computed(() => assets.value?.references.length ?? 0);

const METER_COLORS = [
  dt("blue.600"),
  dt("red.600"),
  dt("yellow.600"),
  dt("green.600"),
  dt("purple.600"),
  dt("orange.600"),
  dt("teal.600"),
  dt("lime.600"),
  dt("pink.600"),
];
const meterItems = computed<MeterItem[]>(() => {
  const assetTypeCounts: Partial<Record<CommonAssetType, number>> = {};

  for (const asset of assets.value?.assets ?? []) {
    if (!assetTypeCounts[asset.type]) {
      assetTypeCounts[asset.type] = 1;
    } else {
      assetTypeCounts[asset.type]!++;
    }
  }

  // Do not display asset types with under 3 percent representation
  const minItemCountForDisplay = Math.floor(assetCount.value * 0.03);

  const assetMeterItems: MeterItem[] = Object.entries(assetTypeCounts)
    .filter((entry) => entry[1] > minItemCountForDisplay)
    .sort((e0, e1) => e1[1] - e0[1])
    .map((entry) => ({
      label: getAssetTypeNameCapitalized(entry[0] as CommonAssetType),
      value: Math.round((entry[1] / assetCount.value) * 100),
    }));

  // Since the PrimeVue component rounds to percent we want to fill up the bar completely
  const otherCount = 100 - assetMeterItems.reduce((val, entry) => val + entry.value, 0);

  if (otherCount > 0) {
    assetMeterItems.push({
      label: "Other",
      value: otherCount,
    });
  }

  return assetMeterItems.map(
    (item, index) =>
      ({
        ...item,
        color: METER_COLORS[index % METER_COLORS.length],
      }) satisfies MeterItem,
  );
});

const selectedZoneDetails = computed<ZoneDto | null>(
  () => zoneStore.loadedZones.find((zone) => zone.name === props.selectedZone) ?? null,
);

watch(
  () => props.selectedZone,
  (newValue) => {
    assets.value = null;
    if (!newValue) return;
    webviewBinds.getAssetsForZone(newValue).then((res) => {
      if (props.selectedZone === newValue) {
        assets.value = res;
      }
    });
  },
  { immediate: true },
);
</script>

<template>
  <div class="zone-details">
    <h2>{{ selectedZone ?? "No zone selected" }}</h2>
    <Button label="Show assets" :disabled="!selectedZone" />
    <div v-if="selectedZoneDetails" class="zone-tags">
      <Tag :value="selectedZoneDetails.game" />
      <Tag :value="selectedZoneDetails.platform" />
    </div>
    <div class="zone-assets">
      <template v-if="assets">
        <div>{{ assetCount }} assets</div>
        <div>{{ referenceCount }} references</div>
        <MeterGroup class="asset-meter" :value="meterItems" />
      </template>
      <template v-else-if="selectedZone">
        <Skeleton class="count-skeleton" width="10em" />
        <Skeleton class="count-skeleton" width="10em" />
        <Skeleton class="count-skeleton" height="0.5lh" />
      </template>
    </div>
  </div>
</template>

<style lang="scss" scoped>
.zone-tags {
  display: flex;
  margin-top: 0.5em;
  column-gap: 0.5em;
  row-gap: 0.5em;
}

.zone-assets {
  margin-top: 0.5em;
  display: flex;
  flex-direction: column;
}

.asset-meter {
  padding-top: 0.5em;
  font-size: 0.8em;
  line-height: 1.25;
  --p-metergroup-gap: 1em;
  --p-metergroup-label-list-horizontal-gap: 0.8em;
}

.count-skeleton {
  margin-bottom: 0.5em;
}
</style>
