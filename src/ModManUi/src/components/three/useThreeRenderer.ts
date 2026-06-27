import {
  type ComponentInstance,
  computed,
  type MaybeRefOrGetter,
  ref,
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
  orthoAdjustForAspect?: boolean;
  orthoWidth?: MaybeRefOrGetter<number>;
  orthoHeight?: MaybeRefOrGetter<number>;
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
  const orthoWidth = options?.orthoWidth ?? 1;
  const orthoHeight = options?.orthoHeight ?? 1;

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

    const orthoWidthValue = toValue(orthoWidth);
    const orthoHeightValue = toValue(orthoHeight);
    return new OrthographicCamera(
      orthoWidthValue / -2,
      orthoWidthValue / 2,
      orthoHeightValue / 2,
      orthoHeightValue / -2,
      toValue(zNear),
      toValue(zFar),
    );
  }

  const camera = shallowRef<CameraType>(createNewCamera());
  const aspect = ref(1);
  let renderer: WebGLRenderer | undefined = undefined;

  watch(
    () => [canvas.value, canvasWrapper.value],
    ([canvasValue, canvasWrapperValue]) => {
      if (!canvasValue || !canvasWrapperValue) return;

      renderer = new WebGLRenderer({ canvas: canvasValue, alpha: true });
      const canvasWrapperBounds = canvasWrapperValue.getBoundingClientRect();
      renderer.setSize(canvasWrapperBounds.width, canvasWrapperBounds.height);
      aspect.value = canvasWrapperBounds.width / canvasWrapperBounds.height;

      if (camera.value instanceof PerspectiveCamera) {
        camera.value.aspect = aspect.value;
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
    aspect.value = canvasWrapperBounds.width / canvasWrapperBounds.height;
  });

  watch(aspect, (newValue) => {
    if (camera.value instanceof PerspectiveCamera) {
      camera.value.aspect = newValue;
      camera.value.updateProjectionMatrix();
    }
  });

  watch(
    () => toValue(fov),
    (fovValue) => {
      const cameraValue = camera.value;
      if (cameraValue instanceof PerspectiveCamera) {
        cameraValue.fov = fovValue;
        cameraValue.updateProjectionMatrix();
      }
    },
  );

  if (options?.orthoAdjustForAspect === true) {
    watch(
      () => [toValue(orthoWidth), toValue(orthoHeight), aspect.value],
      ([orthoWidthValue, orthoHeightValue, aspect]) => {
        const cameraValue = camera.value;

        if (orthoWidthValue > orthoHeightValue) {
          orthoHeightValue = orthoWidthValue / aspect;
        } else {
          orthoWidthValue = orthoHeightValue * aspect;
        }

        if (cameraValue instanceof OrthographicCamera) {
          cameraValue.left = -orthoWidthValue;
          cameraValue.right = orthoWidthValue;
          cameraValue.top = orthoHeightValue;
          cameraValue.bottom = -orthoHeightValue;
          cameraValue.updateProjectionMatrix();
        }
      },
    );
  } else {
    watch(
      () => [toValue(orthoWidth), toValue(orthoHeight)],
      ([orthoWidthValue, orthoHeightValue]) => {
        const cameraValue = camera.value;
        if (cameraValue instanceof OrthographicCamera) {
          cameraValue.left = -orthoWidthValue;
          cameraValue.right = orthoWidthValue;
          cameraValue.top = orthoHeightValue;
          cameraValue.bottom = -orthoHeightValue;
          cameraValue.updateProjectionMatrix();
        }
      },
    );
  }

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
