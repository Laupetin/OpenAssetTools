<script setup lang="ts">
import Skeleton from "primevue/skeleton";
import { useAssetStore } from "@/stores/AssetStore";
import { storeToRefs } from "pinia";
import { ref, watch } from "vue";
import InspectPreview from "./components/InspectPreview.vue";
import InspectAssetDetails from "./components/InspectAssetDetails.vue";
import InspectZoneAssets from "./components/InspectZoneAssets.vue";
import type { AssetDto } from "@/native/AssetBinds.ts";

const assetStore = useAssetStore();

const { assetsOfZone } = storeToRefs(assetStore);

const props = defineProps<{
  zoneName: string;
}>();

const selectedAsset = ref<AssetDto | null>(null);

watch(
  () => props.zoneName,
  (newValue) => {
    assetStore.loadAssetsForZone(newValue);
  },
  { immediate: true },
);
</script>

<template>
  <div class="inspect-details">
    <template v-if="assetsOfZone">
      <InspectPreview class="inspect-area-preview" />
      <InspectAssetDetails :selected-asset="selectedAsset" class="inspect-area-details" />
      <InspectZoneAssets
        v-model:selected-asset="selectedAsset"
        :assets="assetsOfZone.assets"
        class="inspect-area-assets"
      />
    </template>
    <template v-else>
      <div class="skeleton-wrapper">
        <Skeleton class="count-skeleton" width="100%" height="100%" />
      </div>
      <div class="skeleton-wrapper">
        <Skeleton class="count-skeleton" width="100%" height="100%" />
      </div>
      <div class="skeleton-wrapper list">
        <Skeleton v-for="i in 3" :key="i" class="count-skeleton" width="80%" height="1em" />
      </div>
    </template>
  </div>
</template>

<style lang="scss" scoped>
.inspect-details {
  width: 100%;
  height: 100%;

  display: grid;
  grid-template-columns: 30% 1fr;
  grid-template-rows: 30% 1fr;
  grid-template-areas:
    "preview details"
    "assets details";
  gap: 0.5rem;
  padding: 0.5rem;
}

.inspect-area-preview {
  grid-area: preview;
}

.inspect-area-details {
  grid-area: details;
}

.inspect-area-assets {
  grid-area: assets;
}

.skeleton-wrapper {
  &.list > *:not(:first-child) {
    margin-top: calc(1lh - 1em);
  }
}
</style>
