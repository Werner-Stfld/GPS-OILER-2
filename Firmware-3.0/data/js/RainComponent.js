import LabelledInput from "./LabelledInputComponent.js"
export default {
  setup() {

    const {inject, onMounted} = Vue;

    const settings = inject('rain')

    function apply () {
      console.log("apply rain settings: " + JSON.stringify(settings))
    }
    return {
      apply, settings
    }
  },
//     "onThreshold": 150,
//     "offThreshold": 50,
//     "distanceMultiplier": 1.5,
//     "afterRainOilingPulses": 10
  template: 
  `<div> \
    <form @submit.prevent="apply"> \
      <fieldset class="grid-form label-box">\
        <legend>Regen</legend>\
        <labelled-input summary="Einschaltschwelle" desc="Grenzwert Regen erkannt."/>\
        <input id="Einschaltschwelle" type="number" min="10" v-model="settings.onThreshold" name="onThreshold"/>\
        &nbsp;\
        <labelled-input summary="Ausschaltschwelle" desc="Grenzwert Regen nicht erkannt."/>\
        <input id="Ausschaltschwelle" type="number" min="10" v-model="settings.offThreshold" name="offThreshold"/>\
        &nbsp;\
        <labelled-input summary="Distanzfaktor" desc="Regendistanzen werden mit diesem Faktor beim ölen berücksichtigt. Sollte > 1 sein."/>\
        <input id="Distanzfaktor" type="number" min="1.0" v-model="settings.distanceMultiplier" name="distanceMultiplier" />\
        &nbsp;\
        <labelled-input summary="Nachölen" desc="Die Anzahl der Ölpulse um nach dem Regen die Kette zu schmieren."/>\
        <input id="Nachölen" type="number" min='1' inputmode='dec' v-model="settings.afterRainOilingPulses" name="afterRainOilingPulses"/>\
        &nbsp;\
      </fieldset>\
    </form>\
  </div>`,
  components: {
    LabelledInput
  }
}