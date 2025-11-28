import LabelledInput from "./LabelledInputComponent.js"
export default {
  setup() {

    const {inject, onMounted} = Vue;

    const settings = inject('display')

    function apply () {
      console.log("apply display settings: " + JSON.stringify(settings))
    }
    return {
      apply, settings
    }
  },
// {
//     "screen1": 2000,
//     "screen2": 2000,
//     "oilSymbol": 2000
// }
  template: 
  `<div> \
    <form @submit.prevent="apply"> \
      <fieldset class="grid-form label-box">\
        <legend>Anzeige</legend>\
        <labelled-input summary="Anzeige1" desc="Anzeigedauer Bildschirm 1 in Sekunden."/>\
        <input id="Anzeige1" type="number" min="300" v-model="settings.screen1" name="screen1"/>\
        <labelled-input summary="Anzeige2" desc="Anzeigedauer Bildschirm 2 in Sekunden."/>\
        <input id="Anzeige2" type="number" min="300" v-model="settings.screen2" name="screen2"/>\
        <labelled-input summary="Anzeigedauer Ölen" desc="Anzeigedauer Ölen Symbol in Sekunden."/>\
        <input id="Anzeigedauer Ölen" type="number" min="100" v-model="settings.oilSymbol" name="oilSymbol"/>\
        <labelled-input summary="Zeitzone" desc="Einstellen der Zeitzone MEZ:1 und der MEZ Sommerzeit: 2"/>\
        <input id="Zeitzone" type="number" min="-13" max="13" v-model="settings.timeZone" name="timeZone"/>\
      </fieldset>\
    </form>\
  </div>`,
  components: {
    LabelledInput
  }
}