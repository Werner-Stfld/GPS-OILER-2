import LabelledInput from "./LabelledInputComponent.js"
export default {
  setup() {

    const {inject} = Vue;

    const settings = inject('tank')
    async function reset () {
      try {
        console.log("reset")
        const response = await fetch("/api/tank/reset", {
          method: "PUT",
          headers: {
            "Content-Type": "application/json"
          },
          body: "{\"refill\": true}"
        })
        if (response.status === 204) {
          console.log("PUT successfully completed.")
        }
      } catch (error) {
        console.error("Fehler bei PUT: " , error)
      }
    }
    return {
      settings,
      reset
    }
  },
  template: 
  `<div> \
    <form @submit.prevent=""> \
      <fieldset class="grid-form label-box">\
        <legend>Tank</legend>\
        <labelled-input summary="Kapazität" desc="Fassungsvermögen (ml)"/>\
        <input id="Kapazität" type="number" v-model="settings.capacity" min='10' inputmode='dec' name="capacity"/>\
        <labelled-input summary="Inhalt" desc="Aktueller Inhalt (ml). Berechnet aus Anzahl der Pulse und Pulse pro ml."/>\
        <input id="Inhalt" type="number" v-model="settings.content" readonly="true" name="content" />\
        <labelled-input summary="Verbrauch zurücksetzen" desc="Aktueller Inhalt (ml). Aktuellen Inhalt auf Kapazität zurücksetzen."/>\
        <button @click="reset" id="Verbrauch zurücksetzen" type="button" >Rücksetzen</button>\
      </fieldset>\
    </form>\
  </div>`,
  components: {
    LabelledInput
  }
}