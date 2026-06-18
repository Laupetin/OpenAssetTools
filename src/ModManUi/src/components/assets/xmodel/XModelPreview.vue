<script setup lang="ts">
import {
  Scene,
  PerspectiveCamera,
  WebGLRenderer,
  AmbientLight,
  HemisphereLight,
  Box3,
  LoadingManager,
  Loader,
  TextureLoader,
  EquirectangularReflectionMapping,
  SRGBColorSpace,
  type Texture,
} from "three";
import { OrbitControls } from "three/addons/controls/OrbitControls.js";
import { GLTFLoader, type GLTF } from "three/addons/loaders/GLTFLoader.js";
import { DDSLoader } from "three/examples/jsm/loaders/DDSLoader.js";
import type { AssetDto } from "@/native/AssetBinds.ts";
import {
  computed,
  onMounted,
  onUnmounted,
  ref,
  shallowRef,
  toRaw,
  useTemplateRef,
  watch,
} from "vue";
import { useResizeObserver } from "@vueuse/core";
import { ThreeResourceTracker } from "@/components/assets/xmodel/ThreeResourceTracker.ts";

const props = defineProps<{
  asset: AssetDto;
  zoneName: string;
}>();

const canvasWrapperRef = useTemplateRef<HTMLDivElement>("canvas-wrapper");
const canvasRef = useTemplateRef<HTMLCanvasElement>("canvas");

const scene = new Scene();
const camera = new PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);

const color = 0xffffff;
const intensity = 1;
const light = new AmbientLight(color, intensity);
scene.add(light);

const skyColor = 0xb1e1ff; // light blue
const groundColor = 0xb97a20; // brownish orange
const hemisphereLight = new HemisphereLight(skyColor, groundColor, intensity);
scene.add(hemisphereLight);

camera.position.z = 3;

const IMAGE_REGEX = /\.\.[\\/]images[\\/](.+)\.(.+)$/m;
class ProxyImageLoader extends Loader {
  private ddsLoader: DDSLoader;
  constructor() {
    super();
    this.ddsLoader = new DDSLoader();
  }
  load(
    url: string,
    onLoad: (data: unknown) => void,
    onProgress?: (event: ProgressEvent) => void,
    onError?: (err: unknown) => void,
  ) {
    const match = IMAGE_REGEX.exec(url);
    if (!match) {
      onError?.("invalid url");
      return;
    }

    this.ddsLoader.load(
      `modman://localhost/image/dds?zone=${encodeURIComponent(props.zoneName)}&name=${encodeURIComponent(match[1])}`,
      onLoad,
      onProgress,
      onError,
    );
  }
  loadAsync(url: string, onProgress?: (event: ProgressEvent) => void): Promise<unknown> {
    const match = IMAGE_REGEX.exec(url);
    if (!match) {
      return Promise.reject("invalid url");
    }

    return this.ddsLoader.loadAsync(
      `modman://localhost/image/dds?zone=${encodeURIComponent(props.zoneName)}&name=${encodeURIComponent(match[1])}`,
      onProgress,
    );
  }
}
const manager = new LoadingManager();
manager.addHandler(IMAGE_REGEX, new ProxyImageLoader());

const gltfLoader = new GLTFLoader(manager);
const modelUri = computed<string>(
  () =>
    `modman://localhost/xmodel/glb?zone=${encodeURIComponent(props.zoneName)}&name=${encodeURIComponent(props.asset.name)}`,
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

watch(
  modelUri,
  (uri) => {
    isLoading.value = true;
    gltfLoader
      .loadAsync(uri)
      .then((gltf) => (model.value = gltf))
      .finally(() => (isLoading.value = false));
  },
  { immediate: true },
);

let renderer: WebGLRenderer | undefined = undefined;
let controls: OrbitControls | undefined;

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
  camera.position.set(cameraX, middleY, middleZ);
  camera.lookAt(middleX, middleY, middleZ);

  camera.far = Math.max(cameraX + sizeX, sizeY, sizeZ, 1000) * 2;
  camera.updateProjectionMatrix();

  if (controls) {
    controls.target.set(middleX, middleY, middleZ);
    controls.update();
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

onMounted(() => {
  renderer = new WebGLRenderer({ canvas: canvasRef.value! });
  const canvasWrapperBounds = canvasWrapperRef.value!.getBoundingClientRect();
  renderer.setSize(canvasWrapperBounds.width, canvasWrapperBounds.height);

  function animate(time: number) {
    renderer!.render(scene, camera);
  }
  renderer.setAnimationLoop(animate);

  controls = new OrbitControls(camera, canvasRef.value!);
  controls.target.set(0, 0, 0);
  controls.update();
});

onUnmounted(() => {
  if (model.value) {
    resourceTracker.unrefObject(toRaw(model.value).scene);
  }
  if (skybox) {
    skybox.dispose();
  }
});

useResizeObserver(canvasWrapperRef, () => {
  const canvasWrapperBounds = canvasWrapperRef.value!.getBoundingClientRect();
  renderer?.setSize(canvasWrapperBounds.width, canvasWrapperBounds.height);
});
</script>

<template>
  <div class="preview-wrapper" ref="canvas-wrapper">
    <canvas class="preview" ref="canvas" />
    <div v-if="isLoading" class="loading-overlay">
      <span>Loading</span>
    </div>
  </div>
</template>

<style scoped lang="scss">
.preview-wrapper {
  display: flex;
  position: relative;
  width: 100%;
  height: 100%;
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
