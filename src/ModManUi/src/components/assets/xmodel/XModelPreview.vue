<script setup lang="ts">
import {
  AmbientLight,
  HemisphereLight,
  Box3,
  TextureLoader,
  EquirectangularReflectionMapping,
  SRGBColorSpace,
  PerspectiveCamera,
  type Texture,
} from "three";
import type { AssetDto } from "@/native/AssetBinds.ts";
import { computed, onUnmounted, ref, shallowRef, toRaw, useTemplateRef, watch } from "vue";
import { ThreeResourceTracker } from "@/components/three/ThreeResourceTracker.js";
import { provideThreeScene } from "@/components/three/useThreeScene.ts";
import { useThreeRenderer } from "@/components/three/useThreeRenderer.ts";
import { useOrbitControls } from "@/components/three/useOrbitControls.ts";
import ThreeScene from "@/components/three/ThreeScene.vue";
import { getModManUrl } from "@/meta/ModManUrl.ts";
import { useGltfLoader, type GLTF } from "@/components/assets/xmodel/GltfLoader.ts";

const props = defineProps<{
  asset: AssetDto;
  zoneName: string;
}>();

const { scene } = provideThreeScene();
const sceneRef = useTemplateRef("sceneRef");
const { camera } = useThreeRenderer(sceneRef);
const { controls } = useOrbitControls(sceneRef, camera);

const color = 0xffffff;
const intensity = 1;
const light = new AmbientLight(color, intensity);
scene.add(light);

const skyColor = 0xb1e1ff; // light blue
const groundColor = 0xb97a20; // brownish orange
const hemisphereLight = new HemisphereLight(skyColor, groundColor, intensity);
scene.add(hemisphereLight);

camera.value.position.z = 3;

const modelUri = computed<string>(() =>
  getModManUrl(
    `/xmodel/glb?zone=${encodeURIComponent(props.zoneName)}&name=${encodeURIComponent(props.asset.name)}`,
  ),
);
const model = shallowRef<GLTF | undefined>(undefined);
const isLoading = ref(false);
const resourceTracker = new ThreeResourceTracker();
const modelBounds = computed<Box3 | undefined>(() => {
  const modelValue = model.value;
  if (!modelValue) return undefined;

  const box = new Box3();
  box.expandByObject(modelValue.scene);
  return box;
});

const loader = new TextureLoader();
let skybox: Texture | undefined = undefined;
loader.loadAsync("/skybox/citrus_orchard_puresky.jpg").then((res) => {
  skybox = res;
  skybox.mapping = EquirectangularReflectionMapping;
  skybox.colorSpace = SRGBColorSpace;
  scene.background = skybox;
});

const gltfLoader = useGltfLoader(() => props.zoneName);

watch(
  modelUri,
  (uri) => {
    isLoading.value = true;
    gltfLoader.value
      .loadAsync(uri)
      .then((gltf) => (model.value = gltf))
      .finally(() => (isLoading.value = false));
  },
  { immediate: true },
);

function resetCameraPositionForObject() {
  const boundsValue = modelBounds.value;
  if (!boundsValue) return;

  const sizeX = boundsValue.max.x - boundsValue.min.x;
  const sizeY = boundsValue.max.y - boundsValue.min.y;
  const sizeZ = boundsValue.max.z - boundsValue.min.z;
  const middleX = boundsValue.min.x + sizeX / 2;
  const middleY = boundsValue.min.y + sizeY / 2;
  const middleZ = boundsValue.min.z + sizeZ / 2;

  const cameraX = Math.max(sizeY, sizeZ) / 2 + boundsValue.max.x;
  camera.value.position.set(cameraX, middleY, middleZ);
  camera.value.lookAt(middleX, middleY, middleZ);

  if (camera instanceof PerspectiveCamera) {
    camera.far = Math.max(cameraX + sizeX, sizeY, sizeZ, 1000) * 2;
    camera.updateProjectionMatrix();
  }

  const controlsValue = controls.value;
  if (controlsValue) {
    controlsValue.target.set(middleX, middleY, middleZ);
    controlsValue.update();
  }
}

watch(model, (newVal, oldVal) => {
  if (newVal) {
    resourceTracker.refObject(newVal.scene);
  }

  if (oldVal) {
    scene.remove(toRaw(oldVal).scene);
  }

  if (newVal) {
    scene.add(toRaw(newVal.scene));
    resetCameraPositionForObject();
  }

  if (oldVal) {
    resourceTracker.unrefObject(toRaw(oldVal).scene);
  }
});

onUnmounted(() => {
  if (model.value) {
    resourceTracker.unrefObject(toRaw(model.value).scene);
  }
  if (skybox) {
    skybox.dispose();
  }
});
</script>

<template>
  <ThreeScene ref="sceneRef" :loading="isLoading" />
</template>
