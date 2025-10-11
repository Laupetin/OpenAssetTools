<script setup lang="ts">
import { ref } from "vue";
import { webviewBinds } from "./native";

const lastPath = ref("");
const loadingFastFile = ref(false);

async function onOpenFastfileClick() {
  lastPath.value =
    (await webviewBinds.openFileDialog({ filters: [{ name: "Fastfiles", filter: "*.ff" }] })) ?? "";

    loadingFastFile.value = true;
    await webviewBinds.loadFastFile(lastPath.value);
    loadingFastFile.value = false;
}
</script>

<template>
  <main class="container">
    <h1>Welcome to ModMan</h1>
    <small>Nothing to see here yet, this is mainly for testing</small>

    <p>
      <button @click="onOpenFastfileClick">Open fastfile</button>
      <span>The last path: {{ lastPath }}</span>
      <span>Loading: {{ loadingFastFile }}</span>
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
