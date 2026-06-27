<script setup lang="ts">
import Splitter from "primevue/splitter";
import SplitterPanel from "primevue/splitterpanel";
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

const selectedAsset = ref<AssetDto | undefined>(undefined);

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
    <Splitter class="splitter-full-size" :gutter-size="2">
      <SplitterPanel class="splitter-panel" :size="30" :min-size="15">
        <Splitter class="splitter-full-size" layout="vertical" :gutter-size="2">
          <SplitterPanel class="splitter-panel" :size="30">
            <InspectPreview v-if="assetsOfZone" :asset="selectedAsset" :zone-name />
            <div v-else class="skeleton-wrapper">
              <Skeleton class="count-skeleton" width="100%" height="100%" />
            </div>
          </SplitterPanel>
          <SplitterPanel class="splitter-panel" :size="70">
            <InspectZoneAssets
              v-if="assetsOfZone"
              v-model:selected-asset="selectedAsset"
              :assets="assetsOfZone.assets"
              :zone-name="zoneName"
            />
            <div v-else class="skeleton-wrapper list">
              <Skeleton v-for="i in 3" :key="i" class="count-skeleton" width="80%" height="1em" />
            </div>
          </SplitterPanel>
        </Splitter>
      </SplitterPanel>
      <SplitterPanel class="splitter-panel" :size="70">
        <InspectAssetDetails v-if="assetsOfZone" :selected-asset="selectedAsset" />
        <div v-else class="skeleton-wrapper">
          <Skeleton class="count-skeleton" width="100%" height="100%" />
        </div>
      </SplitterPanel>
    </Splitter>
  </div>
</template>

<style lang="scss" scoped>
.inspect-details {
  width: 100%;
  height: 100%;
}

.splitter-full-size {
  width: 100%;
  height: 100%;
}

.splitter-panel {
  padding: 0.25rem;
}

.skeleton-wrapper {
  &.list > *:not(:first-child) {
    margin-top: calc(1lh - 1em);
  }
}
</style>
