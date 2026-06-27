<script setup lang="ts">
import Listbox from "primevue/listbox";
import type { AssetDto } from "@/native/AssetBinds.ts";
import AssetListOption from "@/view/inspect_details/components/AssetListOption.vue";
import { computed, ref } from "vue";
import AssetListFilter, {
  type AssetListFilterData,
} from "@/view/inspect_details/components/AssetListFilter.vue";

const selectedAsset = defineModel<AssetDto | undefined>("selectedAsset", { required: true });
const props = defineProps<{
  assets: AssetDto[];
  zoneName: string;
}>();

const filter = ref<AssetListFilterData>({});

const filteredAssets = computed(() => {
  let result = props.assets;

  const nameFilter = filter.value.searchText;
  const assetTypesToFilter = filter.value.assetTypeFilter;

  if (assetTypesToFilter) {
    result = result.filter((asset: AssetDto) => assetTypesToFilter.includes(asset.type));
  }
  if (nameFilter) {
    result = result.filter((asset: AssetDto) => asset.name.indexOf(nameFilter) >= 0);
  }

  return result;
});
</script>

<template>
  <div class="zone-assets">
    <AssetListFilter v-model="filter" :zone-name="zoneName" />
    <Listbox
      v-model="selectedAsset"
      class="asset-list"
      :options="filteredAssets"
      option-label="name"
      scroll-height="100%"
      :virtual-scroller-options="{ itemSize: 24 }"
    >
      <template #option="{ option }: { option: AssetDto }">
        <AssetListOption :asset="option" />
      </template>
    </Listbox>
  </div>
</template>

<style scoped lang="scss">
.zone-assets {
  display: flex;
  flex-direction: column;
}

.asset-list {
  --p-listbox-option-padding: 0;
  flex-grow: 1;
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
