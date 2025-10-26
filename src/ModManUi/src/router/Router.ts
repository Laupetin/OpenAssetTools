import { createRouter, createWebHashHistory, type RouteRecordRaw } from "vue-router";
import { PAGE } from "./Page";
import ZoneInspector from "@/view/inspect/ZoneInspector.vue";
import InspectDetails from "@/view/inspect_details/InspectDetails.vue";

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
