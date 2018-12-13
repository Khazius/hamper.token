#!/bin/bash
cleos -u https://eos.greymass.com set contract aussiehamper . hamper.wasm hamper.abi
cleos -u https://eos.greymass.com set account permission aussiehamper refund '{"threshold":1,"accounts":[{"permission":{"actor":"aussiehamper","permission":"eosio.code"},"weight":1}]}' active -p aussiehamper
cleos -u https://eos.greymass.com set action permission aussiehamper eosio.token transfer refund

cleos -u https://eos.greymass.com set account permission aussiehamper issuer '{"threshold":1,"accounts":[{"permission":{"actor":"aussiehamper","permission":"eosio.code"},"weight":1}]}' active -p aussiehamper
cleos -u https://eos.greymass.com set action permission aussiehamper aussiehamper transfer issuer

cleos -u https://eos.greymass.com push action aussiehamper create '["aussiehamper","10000000 HAMPR"]' -p aussiehamper
