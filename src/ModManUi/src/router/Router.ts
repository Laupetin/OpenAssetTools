import { createRouter, createWebHashHistory, type RouteRecordRaw } from "vue-router";
import { PAGE } from "./Page";
import ZoneInspector from "@/view/inspect/ZoneInspector.vue";
import InspectDetails from "@/view/inspect_details/InspectDetails.vue";
import type { RouteMeta } from "./RouteMeta";
import InspectDetailsHeader from "@/view/inspect_details/InspectDetailsHeader.vue";

const ROUTES: RouteRecordRaw[] = [
  {
    path: "/",
    children: [
      {
        path: "",
        name: PAGE.INSPECT.SELECT_ZONE,
        component: ZoneInspector,
      },
      {
        path: ":zoneName",
        name: PAGE.INSPECT.ZONE_DETAILS,
        component: InspectDetails,
        meta: {
          backTo: PAGE.INSPECT.SELECT_ZONE,
          headerTitleComponent: InspectDetailsHeader,
        } satisfies RouteMeta,
        props: true,
      },
    ],
  },
];

export function createModManRouter() {
  return createRouter({
    history: createWebHashHistory(),
    routes: ROUTES,
  });
}
