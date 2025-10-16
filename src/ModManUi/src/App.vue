<script setup lang="ts">
import Button from "primevue/button";
import { webviewBinds } from "@/native";
import ZoneSelector from "./components/ZoneSelector.vue";
import { useZoneStore } from "./stores/ZoneStore";

const zoneStore = useZoneStore();

async function openFastFileSelect() {
  return await webviewBinds.openFileDialog({ filters: [{ name: "Fastfiles", filter: "*.ff" }] });
}

async function onOpenFastFileClick() {
  const fastFilePath = await openFastFileSelect();
  if (!fastFilePath) return;

  zoneStore.loadFastFile(fastFilePath);
}
</script>

<template>
  <main class="container">
    <h1>Welcome to ModMan</h1>
    <small>Nothing to see here yet, this is mainly for testing</small>

    <div class="actions">
      <Button label="Load fastfile" @click="onOpenFastFileClick" />
    </div>

    <ZoneSelector />
  </main>
</template>

<style scoped lang="scss">
.actions {
  display: flex;
  justify-content: center;
  column-gap: 0.5em;
  margin-top: 1em;
}

.zone-list {
  display: flex;
  flex-direction: column;
  row-gap: 0.5em;
}

.zone > button {
  margin-left: 0.5em;
}

@starting-style {
  .progressbar {
    opacity: 0;
  }
}

.progressbar {
  position: absolute;
  bottom: 0;
  left: 0;
  right: 0;

  height: 0.4rem;

  transition: opacity 0.2s ease-in-out;
  opacity: 1;
}
</style>
