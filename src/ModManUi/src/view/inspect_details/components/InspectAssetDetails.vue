<script setup lang="ts">
import type { AssetDto } from "@/native/AssetBinds.ts";
import Tag from "primevue/tag";
import { getAssetTypeNameCapitalized } from "@/utils/AssetTypeName.ts";
import { computed } from "vue";

const props = defineProps<{
  selectedAsset: AssetDto | null;
}>();

const assetTypeName = computed(() =>
  props.selectedAsset ? getAssetTypeNameCapitalized(props.selectedAsset.type) : "",
);
</script>

<template>
  <div v-if="selectedAsset">
    <h2 class="asset-header">
      <span>{{ selectedAsset.name }}</span>
      <Tag class="asset-type" :value="assetTypeName" />
    </h2>
    <div>No details available</div>
  </div>
  <div v-else>
    <div>No asset selected</div>
  </div>
</template>

<style scoped lang="scss">
.asset-header {
  display: flex;
  align-items: center;
  margin: 0 0 0.5em 0;
}

.asset-type {
  margin-left: 0.5em;
}
</style>
