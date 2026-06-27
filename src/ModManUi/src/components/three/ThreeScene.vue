<script setup lang="ts">
import { useTemplateRef } from "vue";

withDefaults(
  defineProps<{
    loading?: boolean;
  }>(),
  { loading: false },
);

const canvasWrapperRef = useTemplateRef<HTMLDivElement>("canvas-wrapper");
const canvasRef = useTemplateRef<HTMLCanvasElement>("canvas");

defineExpose({ canvasRef, canvasWrapperRef });
</script>

<template>
  <div class="three-scene" ref="canvas-wrapper">
    <canvas class="three-canvas" ref="canvas" />

    <slot />

    <div v-if="loading" class="loading-overlay">
      <span>Loading</span>
    </div>
  </div>
</template>

<style scoped lang="scss">
.three-scene {
  display: flex;
  position: relative;
  width: 100%;
  height: 100%;
}

.three-canvas {
  border-radius: 0.25rem;
}

@starting-style {
  .loading-overlay {
    opacity: 0;
  }
}

.loading-overlay {
  display: flex;
  justify-content: center;
  align-items: center;
  position: absolute;
  inset: 0;

  background-color: rgba(0, 0, 0, 0.5);

  transition: opacity ease-in-out 500ms;
}
</style>
