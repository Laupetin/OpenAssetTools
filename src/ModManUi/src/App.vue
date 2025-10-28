<script setup lang="ts">
import ModManHeader from "./components/ModManHeader.vue";
</script>

<template>
  <main class="container">
    <ModManHeader />

    <div class="router-wrapper">
      <RouterView v-slot="{ Component }">
        <Transition name="blend">
          <component :is="Component" />
        </Transition>
      </RouterView>
    </div>
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

.router-wrapper {
  position: relative;
  height: 100%;
}

.blend-enter-from,
.blend-leave-to {
  opacity: 0;
}

.blend-enter-active,
.blend-leave-active {
  transition: opacity 0.25s ease-in-out;
}

.blend-leave-active {
  position: absolute;
}
</style>
