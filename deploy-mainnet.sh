#!/bin/bash
cleos -u https://mainnet.genereos.io set contract aussiehamper . hamper.wasm hamper.abi
cleos -u https://mainnet.genereos.io set account permission aussiehamper refund '{"threshold":1,"accounts":[{"permission":{"actor":"aussiehamper","permission":"eosio.code"},"weight":1}]}' active -p aussiehamper
cleos -u https://mainnet.genereos.io set action permission aussiehamper eosio.token transfer refund
cleos -u https://mainnet.genereos.io push action aussiehamper create '["aussiehamper","10000000 HAMPR"]' -p aussiehamper
