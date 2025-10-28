<script setup lang="ts">
import Skeleton from "primevue/skeleton";
import { useAssetStore } from "@/stores/AssetStore";
import { storeToRefs } from "pinia";
import { watch } from "vue";
import InspectPreview from "./components/InspectPreview.vue";
import InspectAssetDetails from "./components/InspectAssetDetails.vue";
import InspectZoneAssets from "./components/InspectZoneAssets.vue";

const assetStore = useAssetStore();

const { assetsOfZone } = storeToRefs(assetStore);

const props = defineProps<{
  zoneName: string;
}>();

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
      <InspectPreview class="inspect-preview" />
      <InspectAssetDetails class="inspect-details" />
      <InspectZoneAssets :assets="assetsOfZone.assets" class="inspect-assets" />
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

.inspect-preview {
  grid-area: preview;
}

.inspect-details {
  grid-area: details;
}

.inspect-assets {
  grid-area: assets;
}

.skeleton-wrapper {
  &.list > *:not(:first-child) {
    margin-top: calc(1lh - 1em);
  }
}
</style>
