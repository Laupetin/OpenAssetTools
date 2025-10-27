import type { RouteLocationNormalizedGeneric } from "vue-router";

export interface TypedRouteMeta {
  nameFunc?(route: RouteLocationNormalizedGeneric): string;
}

export type RouteMeta = TypedRouteMeta & Record<string, unknown>;

export function getNameForRoute(route: RouteLocationNormalizedGeneric): string {
  const meta = route.meta as RouteMeta;

  if (meta.nameFunc) return meta.nameFunc(route);

  return String(route.name);
}
