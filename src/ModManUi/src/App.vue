<script setup lang="ts">
import { onUnmounted, ref } from "vue";
import { webviewBinds, webviewAddEventListener, webviewRemoveEventListener } from "./native";

const greetMsg = ref("");
const lastPersonGreeted = ref("");
const name = ref("");

async function greet() {
  greetMsg.value = await webviewBinds.greet(name.value);
}

function onPersonGreeted(person: string) {
  lastPersonGreeted.value = person;
}

webviewAddEventListener("greeting", onPersonGreeted);

onUnmounted(() => webviewRemoveEventListener("greeting", onPersonGreeted));
</script>

<template>
  <main class="container">
    <h1>Welcome to ModMan</h1>
    <small>Nothing to see here yet, this is mainly for testing</small>

    <form class="row" @submit.prevent="greet">
      <input id="greet-input" v-model="name" placeholder="Enter a name..." autocomplete="off" />
      <button type="submit">Greet</button>
    </form>
    <p>{{ greetMsg }}</p>
    <p>The last person greeted is: {{ lastPersonGreeted }}</p>
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
