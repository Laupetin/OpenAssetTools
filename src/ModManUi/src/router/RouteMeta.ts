import { type Component, computed } from "vue";
import { useRoute, type RouteLocationNormalizedGeneric } from "vue-router";

type HeaderTitleFunc = (route: RouteLocationNormalizedGeneric) => string;
export interface TypedRouteMeta {
  backTo?: string;
  headerTitleFunc?: HeaderTitleFunc;
  headerTitleComponent?: Component;
}

export type RouteMeta = TypedRouteMeta & Record<string, unknown>;

export function useRouteMeta() {
  const route = useRoute();
  const meta = computed<RouteMeta>(() => route.meta as RouteMeta);

  const headerTitle = computed<string | Component>(() => {
    if (meta.value.headerTitleFunc) {
      return meta.value.headerTitleFunc(route);
    } else if (meta.value.headerTitleComponent) {
      return meta.value.headerTitleComponent;
    }

    return String(route.name);
  });

  const routeNavigateBackTo = computed<string | null>(() => meta.value.backTo ?? null);

  return { headerTitle, routeNavigateBackTo };
}
