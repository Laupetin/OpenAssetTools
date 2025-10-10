<script setup lang="ts">
import { ref } from "vue";
import { webviewBinds } from "./native";

const lastPath = ref("");

async function onOpenFastfileClick() {
  lastPath.value =
    (await webviewBinds.openFileDialog({ filters: [{ name: "Fastfiles", filter: "*.ff" }] })) ?? "";
}
</script>

<template>
  <main class="container">
    <h1>Welcome to ModMan</h1>
    <small>Nothing to see here yet, this is mainly for testing</small>

    <p>
      <button @click="onOpenFastfileClick">Open fastfile</button>
      <span>The last path: {{ lastPath }}</span>
    </p>
  </main>
</template>

<style scoped>
.logo.vite:hover {
  filter: drop-shadow(0 0 2em #747bff);
}

.logo.vue:hover {
  filter: drop-shadow(0 0 2em #249b73);
}
</style>
