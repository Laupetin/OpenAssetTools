<script setup lang="ts">
import { computed } from "vue";
import IftaLabel from "primevue/iftalabel";
import InputText from "primevue/inputtext";
import MultiSelect from "primevue/multiselect";
import IconField from "primevue/iconfield";
import InputIcon from "primevue/inputicon";
import IconMagnifyingGlass from "@/components/icons/IconMagnifyingGlass.vue";
import { useZoneStore } from "@/stores/ZoneStore.ts";
import { getAssetTypesByGame } from "@/meta/GameAssetTypes.ts";
import { localizeAssetType } from "@/i18n/i18n.ts";

export interface AssetListFilterData {
  searchText?: string;
  assetTypeFilter?: string[];
}

const modelValue = defineModel<AssetListFilterData>({ required: true });
const props = defineProps<{
  zoneName: string;
}>();
const zoneStore = useZoneStore();
const zone = computed(() => {
  const result = zoneStore.getLoadedZoneByName(props.zoneName);
  if (!result) throw new Error("Could not find zone");
  return result;
});
const gameId = computed(() => zone.value.game);
const availableAssetTypes = computed(() =>
  getAssetTypesByGame(gameId.value).map((assetType) => ({
    value: assetType,
    name: localizeAssetType(assetType),
  })),
);

const searchText = computed<string>({
  get: () => modelValue.value.searchText ?? "",
  set: (value) => {
    if (value) {
      modelValue.value = { ...modelValue.value, searchText: value };
    } else {
      modelValue.value = { ...modelValue.value, searchText: undefined };
    }
  },
});

const assetTypeFilter = computed<string[]>({
  get: () => modelValue.value.assetTypeFilter ?? [],
  set: (value) => {
    if (value.length > 0) {
      modelValue.value = { ...modelValue.value, assetTypeFilter: value };
    } else {
      modelValue.value = { ...modelValue.value, assetTypeFilter: undefined };
    }
  },
});
</script>

<template>
  <div class="filters">
    <IftaLabel>
      <IconField>
        <InputIcon>
          <IconMagnifyingGlass class="search-icon" />
        </InputIcon>
        <InputText v-model="searchText" fluid inputId="alf_search" />
        <InputIcon>
          <IconMagnifyingGlass class="search-icon" />
        </InputIcon>
      </IconField>
      <label for="alf_search">Asset name</label>
    </IftaLabel>
    <IftaLabel>
      <MultiSelect
        v-model="assetTypeFilter"
        fluid
        inputId="alf_asset_types"
        :options="availableAssetTypes"
        show-clear
        option-label="name"
        option-value="value"
        :max-selected-labels="2"
        filter
        variant="filled"
      />
      <label for="alf_asset_types">Asset types</label>
    </IftaLabel>
  </div>
</template>

<style scoped lang="scss">
@use "sass:math";

$FILTER_GAP: 0.5rem;
$FILTER_COL_COUNT: 2;
$FILTER_PAD_MIDDLE: ($FILTER_COL_COUNT - 1) * $FILTER_GAP;
$FILTER_PAD_EDGE: math.div($FILTER_PAD_MIDDLE, 2);

.filters {
  display: grid;
  grid-template-columns: calc(60% - #{$FILTER_PAD_EDGE}) calc(40% - #{$FILTER_PAD_EDGE});
  grid-gap: 0.5rem;
  padding-bottom: 0.5rem;
}

.search-icon {
  height: 1em;
}
</style>
