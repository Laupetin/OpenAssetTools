import "../public/favicon.ico";
import "./main.scss";

import { createApp } from "vue";
import { createPinia } from "pinia";
import { configurePrimeVue } from "./PrimeVue.ts";

import App from "./App.vue";

const app = createApp(App);

app.use(createPinia());

configurePrimeVue(app);

app.mount("#app");
