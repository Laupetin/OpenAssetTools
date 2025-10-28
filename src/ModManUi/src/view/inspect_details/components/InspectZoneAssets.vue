<script setup lang="ts">
import Listbox from "primevue/listbox";
import type { AssetDto } from "@/native/AssetBinds.ts";
import AssetListOption from "@/view/inspect_details/components/AssetListOption.vue";

const selectedAsset = defineModel<AssetDto | null>("selectedAsset");
defineProps<{
  assets: AssetDto[];
}>();
</script>

<template>
  <Listbox
    v-model="selectedAsset"
    class="asset-list"
    :options="assets"
    option-label="name"
    scroll-height="100%"
    :virtual-scroller-options="{ itemSize: 24 }"
  >
    <template #option="{ option }: { option: AssetDto }">
      <AssetListOption :asset="option" />
    </template>
  </Listbox>
</template>

<style scoped lang="scss">
.asset-list {
  --p-listbox-option-padding: 0;
}

:deep(.p-virtualscroller) {
  overflow-x: hidden;
  max-width: 100%;
}

:deep(.p-listbox-list) {
  max-width: 100%;
}

:deep(.p-listbox-list-container) {
  height: 100%;
}
</style>
