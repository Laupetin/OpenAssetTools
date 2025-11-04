import type { App } from "vue";
import PrimeVue from "primevue/config";
import Aura from "@primeuix/themes/aura";
import { definePreset } from "@primeuix/themes";
import type { ColorScale } from "@primeuix/styled";

const ModManTheme = definePreset(Aura, {
  primitive: {
    brand: {
      50: "var(--color-brand-50)",
      100: "var(--color-brand-100)",
      200: "var(--color-brand-200)",
      300: "var(--color-brand-300)",
      400: "var(--color-brand-400)",
      500: "var(--color-brand-500)",
      600: "var(--color-brand-600)",
      700: "var(--color-brand-700)",
      800: "var(--color-brand-800)",
      900: "var(--color-brand-900)",
      950: "var(--color-brand-950)",
    } satisfies ColorScale,
  },
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
        darkModeSelector: ".dark",
      },
    },
  });
  // Always make dark mode for now
  document.documentElement.classList.add("dark");
}
