<script setup lang="ts">
import ModManHeader from "./components/ModManHeader.vue";
</script>

<template>
  <main class="container">
    <ModManHeader />

    <RouterView />
  </main>
</template>

<style scoped lang="scss">
.container {
  margin: 0;
  display: flex;
  position: relative;
  flex-direction: column;

  height: 100vh;
}
</style>
