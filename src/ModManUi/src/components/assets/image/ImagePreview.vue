<script setup lang="ts">
import ThreeScene from "@/components/three/ThreeScene.vue";
import { computed, ref, useTemplateRef, watch } from "vue";
import {
  AmbientLight,
  CompressedTexture,
  DoubleSide,
  HemisphereLight,
  Material,
  Mesh,
  MeshBasicMaterial,
  PlaneGeometry,
  type Texture,
} from "three";
import type { AssetDto } from "@/native/AssetBinds.ts";
import {
  createResourceTrackedObject,
  createSceneObject,
  provideThreeScene,
} from "@/components/three/useThreeScene.ts";
import { useThreeRenderer } from "@/components/three/useThreeRenderer.ts";
import { getModManUrl } from "@/meta/ModManUrl.ts";
import { DDSLoader } from "three/examples/jsm/loaders/DDSLoader.js";

const props = defineProps<{
  asset: AssetDto;
  zoneName: string;
}>();

const { scene } = provideThreeScene();

const color = 0xffffff;
const intensity = 10;
const light = new AmbientLight(color, intensity);
scene.add(light);

const skyColor = 0xb1e1ff; // light blue
const groundColor = 0xb97a20; // brownish orange
const hemisphereLight = new HemisphereLight(skyColor, groundColor, intensity);
scene.add(hemisphereLight);

const imageUri = computed<string>(() =>
  getModManUrl(
    `/image/dds?zone=${encodeURIComponent(props.zoneName)}&name=${encodeURIComponent(props.asset.name)}`,
  ),
);

const isLoading = ref(false);

const loader = new DDSLoader();
const texture = createResourceTrackedObject<Texture>();
watch(
  imageUri,
  (uri) => {
    isLoading.value = true;
    loader
      .loadAsync(uri)
      .then((loadedTexture: CompressedTexture) => {
        loadedTexture.needsUpdate = true;
        texture.value = loadedTexture;
      })
      .finally(() => (isLoading.value = false));
  },
  { immediate: true },
);
const textureWidth = computed(() => texture.value?.width ?? 0);
const textureHeight = computed(() => texture.value?.height ?? 0);

const sceneRef = useTemplateRef("sceneRef");
const { camera } = useThreeRenderer(sceneRef, {
  cameraType: "orthographic",
  orthoAdjustForAspect: true,
  orthoWidth: () => textureWidth.value / 2,
  orthoHeight: () => textureHeight.value / 2,
});
camera.value.position.z = -1;
camera.value.rotation.x = Math.PI;
camera.value.updateProjectionMatrix();

const material = createResourceTrackedObject<Material>();
watch(texture, (newValue) => {
  material.value = new MeshBasicMaterial({ map: newValue, transparent: true, side: DoubleSide });
});

const textureMesh = createSceneObject<Mesh>();
watch(material, (newValue) => {
  const newMesh = new Mesh(new PlaneGeometry(textureWidth.value, textureHeight.value), newValue);
  newMesh.position.x = 0;
  newMesh.position.y = 0;
  newMesh.position.z = 0;
  textureMesh.value = newMesh;
});
</script>

<template>
  <ThreeScene ref="sceneRef" :loading="isLoading" />
</template>
