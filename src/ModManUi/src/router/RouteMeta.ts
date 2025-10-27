import { computed } from "vue";
import { useRoute, type RouteLocationNormalizedGeneric } from "vue-router";

export interface TypedRouteMeta {
  backTo?: string;
  nameFunc?(route: RouteLocationNormalizedGeneric): string;
}

export type RouteMeta = TypedRouteMeta & Record<string, unknown>;

export function useRouteMeta() {
  const route = useRoute();
  const meta = computed<RouteMeta>(() => route.meta as RouteMeta);

  const routeDisplayName = computed<string>(() => {
    if (meta.value.nameFunc) return meta.value.nameFunc(route);

    return String(route.name);
  });

  const routeNavigateBackTo = computed<string | null>(() => meta.value.backTo ?? null);

  return { routeDisplayName, routeNavigateBackTo };
}
