#!/bin/bash
cleos create account eosio eosio.token EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio aussiehamper EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio user1 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio user2 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV

cleos set contract eosio.token . eosio.token.wasm eosio.token.abi
cleos set contract aussiehamper . hamper.wasm hamper.abi
cleos set account permission aussiehamper refund '{"threshold":1,"accounts":[{"permission":{"actor":"aussiehamper","permission":"eosio.code"},"weight":1}]}' active -p aussiehamper
cleos set action permission aussiehamper eosio.token transfer refund

cleos push action eosio.token create '["eosio.token","10000000.0000 EOS"]' -p eosio.token
cleos push action aussiehamper create '["aussiehamper","10000000 HAMPR"]' -p aussiehamper

cleos push action eosio.token issue '["user1","100.0000 EOS","Init"]' -p eosio.token
cleos push action eosio.token issue '["user2","100.0000 EOS","Init"]' -p eosio.token

cleos push action eosio.token transfer '["user1","aussiehamper","5.0000 EOS","Init"]' -p user1
cleos push action eosio.token transfer '["user1","aussiehamper","3.3333 EOS","Init"]' -p user1
