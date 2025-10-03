<script setup lang="ts">
import { ref } from "vue";
import { nativeMethods } from "./native";

const greetMsg = ref("");
const name = ref("");

async function greet() {
  greetMsg.value = await nativeMethods.greet(name.value);
}
</script>

<template>
  <main class="container">
    <h1>Welcome to Webview + Vue</h1>

    <form class="row" @submit.prevent="greet">
      <input id="greet-input" v-model="name" placeholder="Enter a name..." autocomplete="off" />
      <button type="submit">Greet</button>
    </form>
    <p>{{ greetMsg }}</p>
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
