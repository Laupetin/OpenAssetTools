<script setup lang="ts">
import Button from "primevue/button";
import { useRoute, useRouter } from "vue-router";
import IconArrowLeft from "./icons/IconArrowLeft.vue";
import IconGear from "./icons/IconGear.vue";
import { computed } from "vue";
import { getNameForRoute } from "@/router/RouteMeta";

const route = useRoute();
const router = useRouter();

const routeName = computed<string>(() => getNameForRoute(route));

function onClickBack() {
  router.back();
}
</script>

<template>
  <header class="header">
    <div class="header-section left">
      <Button variant="text" severity="secondary" aria-label="Back" @click="onClickBack">
        <template #icon>
          <IconArrowLeft class="icon" />
        </template>
      </Button>
    </div>
    <h1 class="title">{{ routeName }}</h1>
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
  background-color: colors.$CONTENT-BACKGROUND;
  text-align: center;

  padding: 0.25rem 0.25rem;

  display: flex;
  justify-content: space-between;

  border-bottom: 1px solid colors.$CONTENT-BORDER-COLOR;
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
