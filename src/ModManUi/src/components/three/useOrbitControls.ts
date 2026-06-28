import { type ComponentInstance, type MaybeRefOrGetter, shallowRef, toValue, watch } from "vue";
import { OrbitControls } from "three/addons/controls/OrbitControls.js";
import type { Camera } from "three";
import ThreeScene from "@/components/three/ThreeScene.vue";

export function useOrbitControls(
  sceneComponent: MaybeRefOrGetter<ComponentInstance<typeof ThreeScene> | null>,
  camera: MaybeRefOrGetter<Camera>,
) {
  const controls = shallowRef<OrbitControls | undefined>(undefined);

  watch(
    () => toValue(sceneComponent)?.canvasRef ?? null,
    (canvasRefValue) => {
      if (canvasRefValue) {
        const cameraValue = toValue(camera);
        controls.value = new OrbitControls(cameraValue, canvasRefValue);
        controls.value.target.set(0, 0, 0);
        controls.value.update();
      }
    },
    { immediate: true },
  );

  return { controls };
}
