import type { App } from "vue";
import PrimeVue from "primevue/config";
import Aura from "@primeuix/themes/aura";
import { definePreset } from "@primeuix/themes";

const ModManTheme = definePreset(Aura, {
  semantic: {
    primary: {
      50: "{orange.50}",
      100: "{orange.100}",
      200: "{orange.200}",
      300: "{orange.300}",
      400: "{orange.400}",
      500: "{orange.500}",
      600: "{orange.600}",
      700: "{orange.700}",
      800: "{orange.800}",
      900: "{orange.900}",
      950: "{orange.950}",
    },
  },
});

export function configurePrimeVue(app: App) {
  app.use(PrimeVue, {
    theme: {
      preset: ModManTheme,
      options: {
        darkModeSelector: ".dark-mode",
      },
    },
  });
  // Always make dark mode for now
  document.documentElement.classList.add("dark-mode");
}
