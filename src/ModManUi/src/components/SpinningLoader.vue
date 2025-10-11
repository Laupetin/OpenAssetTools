<template>
  <svg viewBox="0 0 100 100" class="loading-spinner">
    <circle class="loading-spinner-box" cx="50" cy="50" r="40" fill="transparent" />
  </svg>
</template>

<style lang="scss" scoped>
.loading-spinner {
  width: 0.8em;
  height: 0.8em;
  display: inline-block;

  animation: rotation 1s infinite linear;

  .loading-spinner-box {
    stroke: currentColor;
    stroke-width: 10;
    stroke-dasharray: 100% 100%;
  }
}

@keyframes rotation {
  0% {
    transform: rotate(0deg);
  }
  100% {
    transform: rotate(360deg);
  }
}
</style>
