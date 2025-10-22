import "../public/favicon.ico";
import "@style/main.scss";

import { createApp } from "vue";
import { createPinia } from "pinia";
import { configurePrimeVue } from "./PrimeVue.ts";

import App from "./App.vue";
import { createModManRouter } from "./router/Router.ts";

const app = createApp(App);

app.use(createPinia());
app.use(createModManRouter());

configurePrimeVue(app);

app.mount("#app");
