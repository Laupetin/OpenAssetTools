<script setup lang="ts">
import ModManHeader from "./components/ModManHeader.vue";
</script>

<template>
  <main class="container">
    <ModManHeader />

    <RouterView v-slot="{ Component }">
      <Transition name="blend">
        <component :is="Component" />
      </Transition>
    </RouterView>
  </main>
</template>

<style scoped lang="scss">
@use "@style/variables";

.container {
  margin: 0;
  display: flex;
  position: relative;
  flex-direction: column;

  height: 100vh;
}

.blend-enter-from,
.blend-leave-to {
  opacity: 0;
}

.blend-enter-active,
.blend-leave-active {
  transition: opacity 0.15s ease-in-out;
}
</style>
