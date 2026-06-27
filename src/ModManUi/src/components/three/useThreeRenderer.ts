import {
  type ComponentInstance,
  computed,
  type MaybeRefOrGetter,
  shallowRef,
  toValue,
  watch,
} from "vue";
import { OrthographicCamera, PerspectiveCamera, WebGLRenderer } from "three";
import { useResizeObserver } from "@vueuse/core";
import ThreeScene from "@/components/three/ThreeScene.vue";
import { useThreeSceneOrThrow } from "@/components/three/useThreeScene.ts";

export type CameraType = PerspectiveCamera | OrthographicCamera;
export interface UseThreeRendererOptions {
  cameraType?: "perspective" | "orthographic";
  fov?: MaybeRefOrGetter<number>;
  orthoHorizontal?: MaybeRefOrGetter<number>;
  orthoVertical?: MaybeRefOrGetter<number>;
  zNear?: MaybeRefOrGetter<number>;
  zFar?: MaybeRefOrGetter<number>;
}

export function useThreeRenderer(
  sceneComponent: MaybeRefOrGetter<ComponentInstance<typeof ThreeScene> | null>,
  options?: UseThreeRendererOptions,
) {
  const cameraType = options?.cameraType ?? "perspective";
  const fov = options?.fov ?? 75;
  const zNear = options?.zNear ?? 0.1;
  const zFar = options?.zFar ?? 1000;
  const orthoHorizontal = options?.orthoHorizontal ?? 1;
  const orthoVertical = options?.orthoVertical ?? 1;

  const { scene } = useThreeSceneOrThrow();
  const canvas = computed(() => toValue(sceneComponent)?.canvasRef ?? null);
  const canvasWrapper = computed(() => toValue(sceneComponent)?.canvasWrapperRef ?? null);

  function createNewCamera() {
    if (cameraType === "perspective") {
      let aspect: number = 1;
      const canvasWrapperValue = canvasWrapper.value;
      if (canvasWrapperValue) {
        const canvasWrapperBounds = canvasWrapperValue.getBoundingClientRect();
        aspect = canvasWrapperBounds.width / canvasWrapperBounds.height;
      }

      return new PerspectiveCamera(toValue(fov), aspect, toValue(zNear), toValue(zFar));
    }

    const orthoHorizontalValue = toValue(orthoHorizontal);
    const orthoVerticalValue = toValue(orthoVertical);
    return new OrthographicCamera(
      -orthoHorizontalValue,
      orthoHorizontalValue,
      orthoVerticalValue,
      -orthoVerticalValue,
      toValue(zNear),
      toValue(zFar),
    );
  }

  const camera = shallowRef<CameraType>(createNewCamera());
  let renderer: WebGLRenderer | undefined = undefined;

  watch(
    () => [canvas.value, canvasWrapper.value],
    ([canvasValue, canvasWrapperValue]) => {
      if (!canvasValue || !canvasWrapperValue) return;

      renderer = new WebGLRenderer({ canvas: canvasValue });
      const canvasWrapperBounds = canvasWrapperValue.getBoundingClientRect();
      renderer.setSize(canvasWrapperBounds.width, canvasWrapperBounds.height);

      if (camera.value instanceof PerspectiveCamera) {
        camera.value.aspect = canvasWrapperBounds.width / canvasWrapperBounds.height;
      }

      function animate() {
        renderer!.render(toValue(scene), camera.value);
      }
      renderer.setAnimationLoop(animate);
    },
    { immediate: true },
  );

  useResizeObserver(canvasWrapper, () => {
    const canvasWrapperBounds = canvasWrapper.value!.getBoundingClientRect();
    renderer?.setSize(canvasWrapperBounds.width, canvasWrapperBounds.height);

    if (camera.value instanceof PerspectiveCamera) {
      camera.value.aspect = canvasWrapperBounds.width / canvasWrapperBounds.height;
      camera.value.updateProjectionMatrix();
    }
  });

  watch(
    () => toValue(fov),
    (fovValue) => {
      const cameraValue = camera.value;
      if (cameraValue instanceof PerspectiveCamera) {
        cameraValue.fov = fovValue;
      }
    },
  );

  watch(
    () => toValue(orthoHorizontal),
    (orthoHorizontalValue) => {
      const cameraValue = camera.value;
      if (cameraValue instanceof OrthographicCamera) {
        cameraValue.left = -orthoHorizontalValue;
        cameraValue.right = orthoHorizontalValue;
      }
    },
  );

  watch(
    () => toValue(orthoVertical),
    (orthoVerticalValue) => {
      const cameraValue = camera.value;
      if (cameraValue instanceof OrthographicCamera) {
        cameraValue.top = orthoVerticalValue;
        cameraValue.bottom = -orthoVerticalValue;
      }
    },
  );

  watch(
    () => toValue(zNear),
    (nearValue) => {
      camera.value.near = nearValue;
    },
  );

  watch(
    () => toValue(zFar),
    (farValue) => {
      camera.value.far = farValue;
    },
  );

  return { camera };
}
