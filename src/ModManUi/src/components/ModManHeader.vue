<script setup lang="ts">
import Button from "primevue/button";
import { useRouter } from "vue-router";
import IconArrowLeft from "./icons/IconArrowLeft.vue";
import IconGear from "./icons/IconGear.vue";
import { useRouteMeta } from "@/router/RouteMeta";

const { routeDisplayName, routeNavigateBackTo } = useRouteMeta();
const router = useRouter();

function onClickBack() {
  if (!routeNavigateBackTo.value) return;

  router.push({ name: routeNavigateBackTo.value });
}
</script>

<template>
  <header class="header">
    <div class="header-section left">
      <Button
        v-if="routeNavigateBackTo"
        variant="text"
        severity="secondary"
        aria-label="Back"
        @click="onClickBack"
      >
        <template #icon>
          <IconArrowLeft class="icon" />
        </template>
      </Button>
    </div>
    <h1 class="title">{{ routeDisplayName }}</h1>
    <div class="header-section right">
      <Button variant="text" severity="secondary" aria-label="Settings">
        <template #icon>
          <IconGear class="icon" />
        </template>
      </Button>
    </div>
  </header>
</template>

<style scoped lang="scss">
@use "@style/colors";

.header {
  background-color: colors.$color-content-background;
  text-align: center;

  padding: 0.25rem 0.25rem;

  display: flex;
  justify-content: space-between;

  border-bottom: 1px solid colors.$color-content-border;
}

.header-section {
  width: 12em;

  &.left {
    text-align: left;
  }
  &.right {
    text-align: right;
  }
}

.title {
  font-size: 1rem;
  line-height: 1;
}
</style>
