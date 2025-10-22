import { createRouter, createWebHashHistory, type RouteRecordRaw } from "vue-router";
import { PAGE } from "./Page";
import ZoneInspector from "@/view/inspect/ZoneInspector.vue";

const ROUTES: RouteRecordRaw[] = [
  {
    path: "/",
    name: PAGE.INSPECT_ZONE,
    component: ZoneInspector,
  },
];

export function createModManRouter() {
  return createRouter({
    history: createWebHashHistory(),
    routes: ROUTES,
  });
}
