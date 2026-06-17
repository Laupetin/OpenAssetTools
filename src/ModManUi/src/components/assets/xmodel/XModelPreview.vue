<script setup lang="ts">
import {
  Scene,
  PerspectiveCamera,
  WebGLRenderer,
  BoxGeometry,
  Mesh,
  MeshPhongMaterial,
  AmbientLight,
  HemisphereLight,
  DoubleSide,
} from "three";
import { OrbitControls } from "three/addons/controls/OrbitControls.js";
import type { AssetDto } from "@/native/AssetBinds.ts";
import { onMounted, useTemplateRef } from "vue";

defineProps<{
  asset: AssetDto;
}>();

const canvasRef = useTemplateRef<HTMLCanvasElement>("canvas");

const scene = new Scene();
const camera = new PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);

const geometry = new BoxGeometry(1, 1, 1);
const material = new MeshPhongMaterial({ color: 0x00ff00, side: DoubleSide });
const cube = new Mesh(geometry, material);
scene.add(cube);

const color = 0xffffff;
const intensity = 1;
const light = new AmbientLight(color, intensity);
scene.add(light);

const skyColor = 0xb1e1ff; // light blue
const groundColor = 0xb97a20; // brownish orange
const hemisphereLight = new HemisphereLight(skyColor, groundColor, intensity);
scene.add(hemisphereLight);

camera.position.z = 3;

onMounted(() => {
  const renderer = new WebGLRenderer({ canvas: canvasRef.value! });
  const canvasBounds = canvasRef.value!.getBoundingClientRect();
  renderer.setSize(canvasBounds.width, canvasBounds.height);

  function animate(time: number) {
    renderer.render(scene, camera);
  }
  renderer.setAnimationLoop(animate);

  const controls = new OrbitControls(camera, canvasRef.value!);
  controls.target.set(0, 0, 0);
  controls.update();
});
</script>

<template>
  <canvas class="preview" ref="canvas" />
</template>

<style scoped lang="scss">
.preview {
  width: 100%;
  height: 100%;
}
</style>
